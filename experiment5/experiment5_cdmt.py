import subprocess
import os 
import sys
import time

def main():
    run_cdmt()

def run_cdmt():
    images = ['deepmind', 'django', 'mysql', 'nginx', 'postgres', 'pytorch', 'golang', 'node', 'tomcat', 'tensorflow', 'r-base', 'python', 'redis', 'rails']
    time.sleep(5)
    prefix = '/home/yutan/cdmt/tmp/'
    for image in images:
        eprint(image)
        path = prefix + image
        start = time.time_ns()
        os.system('/home/yutan/cdmt/experiment4/cdmt4/main ' + image) 
        end = time.time_ns()
        with open("/home/yutan/cdmt/results/experiment5/cdmttime.txt") as f:
            f.write(image)
            f.write(':')
            f.write(str(end - start))
            f.write(',')
       
            
