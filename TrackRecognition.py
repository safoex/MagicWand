import re
import matplotlib.pyplot as plt
import matplotlib as mpl
from scipy import signal
import fusion
import time
import numpy as np
import tensorflow as tf
import pickle


def signal_filter(data, x1, x2):
    data = data.transpose()
    res = np.zeros_like(data)
    for i, x in enumerate(data):
        b, a = signal.butter(x1, x2)
        y = signal.filtfilt(b, a, x)
        res[i] = y
    return res.transpose()


def discretize_data(data, n_points):
    step = len(data) / n_points
    data = data.transpose()
    res = np.zeros((data.shape[0], n_points))
    for i, x in enumerate(data):
        for j in range(n_points):
            res[i, j] = np.mean(data[i, int(j * step):int((j+1) * step)])
    return res.transpose()


def new_recognize_track(file_name):
    n_points = 15
    with open('best_classifier.pkl', 'rb') as fid:
        clf = pickle.load(fid)

    row_data = []
    n_lines = 0
    with open(file_name, 'r') as f:
        for line in f:
            result = re.findall(r"-?\d+.\d+", line)
            if len(result) == 10:
                row_data.append([float(x) for x in result])
                n_lines += 1
    if n_lines < n_points:
        return None

    smooth_data = signal_filter(np.array(row_data)[:, 1:7], 3, 0.2)
    data = discretize_data(smooth_data, n_points)

    x = [data.flatten()]
    return clf.predict(x)[0]


def recognize_track(file_name):
    data = []
    with open(file_name, 'r') as f:
        for line in f:
            result = re.findall(r"-?\d+.\d+", line)
            if len(result) == 10:
                data.append([float(x) for x in result])

    fusioner = fusion.Fusion()
    dtime = 0.02
    angles = []
    for a in data:
        fusioner.update_nomag(dtime, (a[1], a[2], a[3]), (a[4], a[5], a[6]))
        angles.append([fusioner.pitch, fusioner.heading, fusioner.roll])
    angles.pop(0)

    data_x = [x[1] for x in angles]
    data_y = [x[0] for x in angles]

    n = 15
    max_x = max(data_x)
    max_y = max(data_y)
    min_x = min(data_x)
    min_y = min(data_y)
    delta = max(max_x - min_x, max_y - min_y) / (n - 1)
    points = set()
    for x, y in zip(data_x, data_y):
        points.add(((int((x - min_x) / delta)), (int((max_y - y) / delta))))

    img = np.zeros([n, n])
    for point in points:
        img[point[1], point[0]] = 1

    img_size = 15
    num_labels = 6
    n_layers = 10
    type_name = {
        0: "circles",
        1: "vertical_lines",
        2: "horizontal_lines",
        3: "zeros",
        4: "up_triangles",
        5: "down_triangles"
    }
    data = np.ndarray([1, img_size * img_size]).astype(np.float32)
    data[0, :] = (img - 0.5).reshape(img_size * img_size).astype(np.float32)

    graph = tf.Graph()
    with graph.as_default():
        weights_1 = tf.Variable(
            tf.truncated_normal([img_size * img_size, n_layers]), name="weights_1")
        weights_2 = tf.Variable(
            tf.truncated_normal([n_layers, num_labels]), name="weights_2")
        biases_1 = tf.Variable(tf.zeros([n_layers]), name="biases_1")
        biases_2 = tf.Variable(tf.zeros([num_labels]), name="biases_2")

        saver = tf.train.Saver()
        prediction = tf.nn.softmax(
            tf.matmul(tf.nn.relu(tf.matmul(data, weights_1) + biases_1), weights_2) + biases_2)

    with tf.Session(graph=graph) as session:
        tf.global_variables_initializer().run()
        saver.restore(session, "tmp/model.ckpt")
        print("Model restored.")
        print('Initialized')
        print(prediction.eval())
        predict_index = np.argmax(prediction.eval(), 1)[0]
        print("Predict class type is " + type_name[predict_index])
    return predict_index
