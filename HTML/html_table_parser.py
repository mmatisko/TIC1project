from bs4 import BeautifulSoup
import pandas as pd
'''
raw_data = {'first_name': ['Jason', 'Molly', 'Tina', 'Jake', 'Amy'],
        'last_name': ['Miller', 'Jacobson', 'Ali', 'Milner', 'Cooze'],
        'age': [42, 52, 36, 24, 73],
        'preTestScore': [4, 24, 31, 2, 3],
        'postTestScore': [25, 94, 57, 62, 70]}
raw_df = pd.DataFrame(raw_data, columns = ['first_name', 'last_name', 'age', 'preTestScore', 'postTestScore'])
'''
text = open("log_example.txt", "r")
html = open('test.html', 'w')

#table variables
threads = []
filter_accuracy = []
lenght = []
first_pair = []
second_pair = []
time = []
filter_io = []
array_io = []

soup = BeautifulSoup(text, 'lxml')
table = soup.find(class_='dataframe')

# Find all the <tr> tag pairs, skip the first one, then for each.
for row in table.find_all('tr')[1:]:
    # Create a variable of all the <td> tag pairs in each <tr> tag pair,
    col = row.find_all('td')

    # Create a variable of the string inside 1st <td> tag pair,
    column_1 = col[0].string.strip()
    threads.append(column_1)

    # Create a variable of the string inside 2nd <td> tag pair,
    column_2 = col[1].string.strip()
    filter_accuracy.append(column_2)

    # Create a variable of the string inside 3rd <td> tag pair,
    column_3 = col[2].string.strip()
    lenght.append(column_3)

    # Create a variable of the string inside 4th <td> tag pair,
    column_4 = col[3].string.strip()
    first_pair.append(column_4)

    # Create a variable of the string inside 4th <td> tag pair,
    column_5 = col[4].string.strip()
    second_pair.append(column_5)

    # Create a variable of the string inside 5th <td> tag pair,
    column_6 = col[5].string.strip()
    time.append(column_6)

# Create a variable of the string inside 5th <td> tag pair,
    column_7 = col[6].string.strip()
    filter_io.append(column_7)

# Create a variable of the string inside 5th <td> tag pair,
    column_8 = col[7].string.strip()
    array_io.append(column_8)

# Create a variable of the value of the columns
columns = {'threads': threads, 'filter_accuracy': filter_accuracy, 'lenght': lenght, 'first_pair': first_pair, 'second_pair': second_pair, 'time': time, 'filter_io': filter_io, 'array_io': array_io, }

# Create a dataframe from the columns variable
df = pd.DataFrame(columns)

print(df)
html.close()
text.close()
