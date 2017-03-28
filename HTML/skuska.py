import pandas as pd
import re
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

# Load files
try:
    text = open("graph_log.txt", "r")
except IOError:
    print('The file does not exist.')
html_i = open('HTML.html', 'w')
html_o = open('test.html', 'w')


# Table variables + data
threads = []
filter_accuracy = []
lenght = []
first_pair1 = []
first_pair2 = []
second_pair1 = []
second_pair2 = []
time = []
filter_io = []
array_io = []

data = {'Threads': threads,
        'Filter accuracy': filter_accuracy,
        'Lenght': lenght,
        'First pair 1': first_pair1,
        'First pair 2': first_pair2,
        'Second pair 1': second_pair1,
        'Second pair 2': second_pair2,
        'Time': time,
        'Filter io': filter_io,
        'Array io': array_io}

# Parsing text file by regular expression
for line in text:
    p_threads = re.search('threads=([0-9]\+[0-9])', line)
    p_filter_accuracy = re.search('filter_accuracy=([0-9]+.+[0-9])', line)
    p_lenght = re.search('lenght=([0-9]+)', line)
    p_first_pair1 = re.search('first_pair=(?P<input>[0-9a-f]+)\s(?P<output>[0-9a-f]+)', line)
    p_second_pair1 = re.search('second_pair=(?P<input>[0-9a-f]+)\s(?P<output>[0-9a-f]+)', line)
    p_time = re.search('time=([0-9]+.+[0-9])', line)
    p_filter_io = re.search('filter_io=([0-9]+)', line)
    p_array_io = re.search('array_io=([0-9]+)', line)

    if p_threads:
        new = p_threads.group(1)
        threads.append(new)
    if p_filter_accuracy:
        new = p_filter_accuracy.group(1)
        filter_accuracy.append(new)
    if p_lenght:
        new = p_lenght.group(1)
        lenght.append(new)
    if p_first_pair1:
        new = p_first_pair1.group(1)
        first_pair1.append(new)
        new2 = p_first_pair1.group(2)
        first_pair2.append(new2)
    if p_second_pair1:
        new = p_second_pair1.group('input')
        second_pair1.append(new)
        new2 = p_second_pair1.group('output')
        second_pair2.append(new2)
    if p_time:
        new = p_time.group(1)
        time.append(new)
    if p_filter_io:
        new = p_filter_io.group(1)
        filter_io.append(int(new))
    if p_array_io:
        new = p_array_io.group(1)
        array_io.append(new)

# HTML part table saves to HTML.html //Better version\\
raw_df = pd.DataFrame(data, columns=['Threads', 'Filter accuracy', 'Lenght', 'First pair 1', 'First pair 2',
                                     'Second pair 1', 'Second pair 2', 'Time', 'Filter io', 'Array io'])
raw_df.to_html(html_i, col_space=80, justify='center', border=5)


# HTML part table 2 saves to test.html
def hover(hover_color="#ffff99"):
    return dict(selector="tr:hover",
                props=[("background-color", "%s" % hover_color)])

styles = [
    hover(),
    dict(selector="table", props=[("border", "2px"),
                                  ("bordercolor", "black")]),
    dict(selector="th", props=[("font-size", "150%"),
                               ("text-align", "center")]),
    dict(selector="caption", props=[("caption-side", "bottom")]),
    dict(selector="tr", props=[("text-align", "center")])

]
html = (raw_df.style.set_table_styles(styles))
print(type(html))
html_o.write(html.render())

# Plot the pandas table number of generated hashes to length of input
matplotlib.style.use('ggplot')
series = pd.Series(filter_io, index=lenght, name='Number of generated hashes to the length of input')
s = series.plot.pie(figsize=(20, 10))
se = s.get_figure()
se.savefig('asd.png')
