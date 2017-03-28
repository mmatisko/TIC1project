from flask import Flask, render_template
import os

app = Flask(__name__)


@app.route('/')
def script_out():
    #aaa = 'ahoj'
    return render_template('HTML.html', aaa='text')

if __name__ == "__main__":
    app.run(debug=True)