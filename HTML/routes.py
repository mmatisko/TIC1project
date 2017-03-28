from flask import Flask, render_template
import os

app = Flask(__name__)


@app.route('/')
def script_out():
    out = execfile("graphs.js")
    return render_template('test.html', out)
