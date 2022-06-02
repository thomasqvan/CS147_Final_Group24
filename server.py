from flask import Flask
from flask import request
import matplotlib.pyplot as plt
import time
import csv
import numpy as np

app = Flask(__name__)

@app.route("/")
def hello():
    print(request.args.get("var"))

    with open('testing.csv', 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([int(request.args.get("var") or 0)])

    return "We received value: "+str(request.args.get("var"))
