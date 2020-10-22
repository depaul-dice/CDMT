import sys
import os
import shutil

def main(directory):
    for filename in os.listdir(directory):
        if filename[-4:] == ".tar":
            continue
        src = directory + '/' + filename
        dest = src + '.tar'
        os.system("tar -cvf " + dest + ' ' + src) 
        shutil.rmtree(src)

if __name__ == "__main__":

    images = ['golang', 'node', 'tomcat', 'tensorflow', 'r-base', 'python', 'redis', 'rails', 'deepmind', 'django', 'mysql', 'nginx', 'pytorch']
    
    prefix = '/home/yutan/cdmt/data/'
    for image in images:
        image = prefix + image
        main(image)
