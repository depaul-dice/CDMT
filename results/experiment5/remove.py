import os 

def remove_all(directory):
    for file in os.listdir(directory):
        if not os.path.isfile(file):
            os.remove(directory + '/' + file)

for directory in os.listdir('.'):
    print(directory)
    if directory == '.' or directory == '..':
        continue
    elif os.path.isdir(directory):
        remove_all(directory)
