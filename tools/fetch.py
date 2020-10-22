import os
import sys
import shutil
import subprocess
import io 

def cp_layer(src, destbase):
    tmp = src[:src.rfind("/")]
    if src[src.rfind("/"):] != "/diff" and src[src.rfind("/"):] != "/diff\n":
        print("could not find the src")
        print("%r"%src[src.rfind("/"):])
        assert False
    tmp = tmp[tmp.rfind("/"):]

    dest = destbase + tmp
    #copytree(src, dest, ignore_dangling_symlinks = True)
    if src.rfind("\n") != -1:
        src = src[:src.rfind("\n")]
    order = "cp -r " + src + " " + dest
    print("this is destination")
    print(dest)
    os.system("mkdir -p " + dest)
    os.system(order) 
    os.system("tar -cvf " + dest + ".tar " + dest)
    shutil.rmtree(dest)
    
def parse_layers(input_, destbase):
    count = 0
    index = input_.find(":")
    while index != -1:
        src = input_[:index]
        input_ = input_[index + 1:]
        index = input_.find(":")
        cp_layer(src, destbase)
    src = input_
    print(destbase)
    cp_layer(src, destbase)

def make_image_dir(imagename):
    os.system("mkdir -p " + imagename)

def parse_images(filename, limit):
    count = 0
    with open(filename, 'r') as f:
        for line in f:
            if limit <= count:
                break
            print(line)
            words = line.split() 
            id_ = words[0]
            image = words[1]
            tag = words[2]
            currdir = image + "/" + tag
            make_image_dir(currdir)
            
            order = "docker inspect -f '{{.GraphDriver.Data.LowerDir}}:{{.GraphDriver.Data.UpperDir}}' " + id_
            
            result = os.popen(order).read()
            parse_layers(result, currdir)
            count += 1
            #os.system("tar -cvf" + currdir + ".tar " + currdir)

def pull_images(imagename, wd):
    os.system("cd " + wd)
    os.system("docker pull " + imagename + " --all-tags")    
    os.system("mkdir -p tmp")
    imagefile = "tmp/" + sys.argv[1] + "/images.txt"
    os.system("docker images --format \"{{.ID}} {{.Repository}} {{.Tag}}\" > " + imagefile)

def mktmp():
    imagefile = "tmp/" + sys.argv[1] + "/images.txt"
    # os.system("docker images --format \"{{.ID}} {{.Repository}} {{.Tag}}\" > " + imagefile)
    result = subprocess.run(['docker', 'images', '--format', '{{.ID}} {{.Repository}} {{.Tag}}'], stdout=subprocess.PIPE)
    msg = result.stdout.decode('ascii')
    buf = io.StringIO(msg)
    with open(imagefile, 'w') as f:
        while True:
            line = buf.readline()
            if len(line) == 0:
                break
            if line.split()[1] == sys.argv[1]:
                # print(line)
                f.write(line)

def main():
    if len(sys.argv) > 1:
        imagefile = "tmp/" + sys.argv[1]+ "/images.txt"
    if len(sys.argv) == 3:
        limit = int(sys.argv[2]) 
    else:
        limit = float('inf')
    if len(sys.argv) > 3:
        assert False
        
    #pull_images(sys.argv[1], sys.argv[2])
    mktmp()
    parse_images(imagefile, limit)

if __name__ == "__main__":
    main()
