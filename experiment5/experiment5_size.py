import os
import sys

def get_size(path, image):
    total_size = 0
    realpath = path + image
    for path, dirs, files in os.walk(realpath):
        for f in files:
            fp = os.path.join(path, f)
            total_size += os.path.getsize(fp)
    return total_size  


def main():
    path = "/home/yutan/cdmt/data/"
    images = ['tomcat', 'deepmind', 'django', 'mysql', 'nginx', 'postgres', 'pytorch', 'golang', 'node', 'tensorflow', 'r-base', 'python', 'redis', 'rails']
    with open("/home/yutan/cdmt/results/experiment5/size.txt", "w") as f:
        for image in images:
            total_size = get_size(path, image)
            f.write(image)
            f.write(':')
            f.write(str(total_size))
            f.write(',')

if __name__ == "__main__":
    main()
