import pandas as pd
from highcharts import Highchart


text = open("log_example.txt", "r")
html_i = open('HTML.html', 'r')
html_o = open('test.html', 'w')

#table variables
threads = []
filter_accuracy = []
lenght = []
first_pair = []
second_pair = []
time = []
filter_io = []
array_io = []

#parser
for line in text:
    if 'threads' in line:
        a = line[8:].split()
        threads.append(a)
    if 'filter_accuracy' in line:
        b = line[16:].split()
        filter_accuracy.append(b)
    if 'lenght' in line:
        c = line[7:].split()
        lenght.append(c)
    if 'first_pair' in line:
        d = line[11:].split()
        first_pair.append(d)
    if 'second_pair' in line:
        e = line[12:].split()
        second_pair.append(e)
    if 'time' in line:
        f = line[5:].split()
        time.append(f)
    if 'filter_io' in line:
        g = line[10:].split()
        filter_io.append(g)
    if 'array_io' in line:
        h = line[9:].split()
        array_io.append(h)

data = {'Threads': threads,
        'Filter accuracy': filter_accuracy,
        'Lenght': lenght,
        'First pair': first_pair,
        'Second pair': second_pair,
        'Time': time,
        'Filter io': filter_io,
        'Array io': array_io}

raw_df = pd.DataFrame(data, columns = ['Threads', 'Filter accuracy', 'Lenght', 'First pair', 'Second pair', 'Time', 'Filter io', 'Array io'])
raw_df.to_html(html_o)






