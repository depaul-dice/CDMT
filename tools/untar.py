import sys
import os
import shutil

def main(directory):
    for filename in os.listdir(directory):
        src = directory + '/' + filename
        if src[-4:] != ".tar":
            print("not untarring " + src)
            continue
        os.system("tar -xvf " + src) 
        os.remove(src)

if __name__ == "__main__":
    images = ['golang', 'node', 'tomcat', 'tensorflow', 'r-base', 'python', 'redis', 'rails', 'deepmind', 'django', 'mysql', 'nginx', 'pytorch']
    for image in images:
        main(image)
