from typing import Tuple

import numpy as np
import pandas as pd
import requests
import _pickle as pkl

from sklearn.preprocessing import StandardScaler, LabelEncoder


class ExperimentalArduinoApi:
    """
    Experimental API for arduino controller over HTTP
    """

    def __init__(self, host, port=80, num_outputs=4, scaler_path='scaler_v1.pkl', labels_path='labels_v1.pkl'):
        """
        :param host: The controller address
        :param port: The controller web port
        :param num_outputs: Expected number of outputs
        """
        self.host = host
        self.port = port
        self.num_outputs = num_outputs

        with open(scaler_path, 'rb') as f:
            self.sc: StandardScaler = pkl.load(f)
        with open(labels_path, 'rb') as f:
            self.le: LabelEncoder = pkl.load(f)

    def predict(self, *x: float, blink=False) -> Tuple[float, float, float, float]:
        """
        Predict single sample.
        :param x: 24-features value of single sample (arg count == 24)
        :param blink: Do blink on the controller based on the output.
        Slows down the execution time by 0.6 - 0.6 * n seconds
        :return: n-class probability tuple (n float tuple)
        :raises HTTP errors: 4xx, 5xx, connection issues. 400 error raises ValueError.
        """
        x = list(x)
        x = pd.DataFrame([pd.Series(data=x, index=[f'Channel {i + 1:d}' for i in range(len(x))])])
        x = self.sc.transform(x)[0]

        params = {f'x{idx + 1}': float(xi) for idx, xi in enumerate(x)}
        if blink: params['blink'] = 1

        resp = requests.get(f'http://{self.host}/predict', params=params)
        if resp.status_code == 400:
            raise ValueError('Invalid input arguments. Detail: ' + resp.text)
        resp.raise_for_status()

        # noinspection PyTypeChecker
        return tuple(float(y) for idx, y in enumerate(str(resp.text).split(' ')) if idx < self.num_outputs)

    def get_label(self, y):
        return self.le.inverse_transform([np.argmax(y)])[0]

    def test_connection(self):
        """
        Tests the connection by predicting a hardcoded sample.
        No error raised means that the test passed.
        :return: The test sample's result
        :raises Exception detailing the test fault
        """
        result = self.predict(
            -0.52109855, -0.57133687, -0.481523, -0.5882358, -0.80570453, -0.03607523,
            -0.8392386, -0.8549552, 0.3732829, -0.5899839, 0.38921237, 0.5864193,
            1.0258859, -0.41387412, -0.5701152, -0.6521362, -0.8420634, -0.68346435,
            -0.8151803, -0.668313, 0.6052867, 1.0809759, 0.77627265, -0.77486044,
        )
        assert all(x >= 0 for x in result)
        assert len(result) == self.num_outputs
        return result
