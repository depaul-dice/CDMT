from __future__ import print_function
import subprocess
import os
import sys
import time

def main():
    run_vvpkg()

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def run_vvpkg():
    images = ['tomcat', 'deepmind', 'django', 'mysql', 'nginx', 'postgres', 'pytorch', 'golang', 'node', 'tensorflow', 'r-base', 'python', 'redis', 'rails']
    time.sleep(5)
    for image in images:
        prefix = "/home/yutan/cdmt/data/"
        path = prefix + image
        os.system("mkdir -p /home/yutan/cdmt/results/experiment5/" + image)
        with open("/home/yutan/cdmt/results/experiment5/" + image + '/hashtime.txt', "w") as f:
            hash_start = time.time_ns() 
            for layer in os.listdir(path):
                if not os.path.isfile(image + '/' + layer):
                    continue
                eprint(image + '/' + layer)
                hash_order = ['../vvpkg/demo/dump_blocks', path + '/' + layer]
                #hash_process = subprocess.Popen(hash_order, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                os.system('../vvpkg/demo/dump_blocks ' + path +'/' + layer)
            hash_end = time.time_ns() 
            f.write(image)
            f.write(':')
            f.write(str(hash_end - hash_start))
            .write(',')

if __name__ == "__main__":
    main()
