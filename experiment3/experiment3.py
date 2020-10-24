import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def main():
    if os.path.exists('merkleposcompare.csv'):
        os.remove('merkleposcompare.csv')
    images = ['golang', 'node', 'tomcat', 'tensorflow', 'deepmind', 'django', 'httpd', 'mysql', 'nginx', 'node', 'postgres', 'pytorch', 'python', 'rails', 'r-base', 'redis']
    for image in images:
        # setup(image)
        run_image(image)
        if not os.path.exists('merkleposcompare.csv'):
            raise Exception('merkleposcompare.csv has to exist')
        
        old, new, mt_changed, pos_changed, mt_total, pos_total = parse()
        assert len(old) == len(new) and len(new) == len(mt_changed) and len(mt_changed) == len(pos_changed) and len(pos_changed) == len(mt_total) and len(mt_total) == len(pos_total) 

        with open('/home/yutan/cdmt/results/experiment3/' + image + '.txt','a+') as f:
            for i in range(len(old)):
               f.write(old[i])
               f.write(':')
               f.write(new[i])
               f.write(':')
               f.write(str(mt_changed[i]))
               f.write(':')
               f.write(str(pos_changed[i]))
               f.write(':')
               f.write(str(mt_total[i]))
               f.write(':')
               f.write(str(pos_total[i]))
               f.write('\n')

        # graph(old, new, mt_changed, pos_changed, mt_total, pos_total)

def run_image(image: str):
    executable = "cdmtvsmerkle/main "
    directory = image
    order = executable + directory
    os.system(order)

def setup(image):
    os.system("bash setup3.sh " + image)

def parse():
    df = pd.read_csv("merkleposcompare.csv", header = None);
    res = df.values
    old = list(); new = list()
    mt_total = list(); pos_total = list()
    mt_changed = list(); pos_changed = list();

    res = res[0];
    for text in res: 
        count = 0
        if isinstance(text, str):
            # print(text)
            while text.find(":") != -1:
                element = text[:text.find(':')]
                text = text[text.find(':') + 1:]
                if count == 0:
                    old.append(element)
                elif count == 1:
                    new.append(element)
                elif count == 2:
                    mt_changed.append(int(element))
                elif count == 3:
                    pos_changed.append(int(element))
                elif count == 4:
                    mt_total.append(int(element))
                elif count == 5:
                    pos_total.append(int(element));
                count += 1
            assert count == 5
            pos_total.append(int(text))
        else:
            print(text)
    return old, new, mt_changed, pos_changed, mt_total, pos_total

def graph(old, new, mt_changed, pos_changed, mt_total, pos_total):
    mt_total = np.array(mt_total)
    pos_total = np.array(pos_total)
    mt_changed = np.array(mt_changed)
    pos_changed = np.array(pos_changed)
    mt_dedup = 1 - mt_changed/mt_total
    pos_dedup = 1 - pos_changed/pos_total
    x = np.arange(len(mt_total)) + 1
    fig, ax = plt.subplots(figsize = (12, 7))
    plt.gcf().subplots_adjust(bottom=0.25)
    ax.bar(x-0.1, mt_dedup, width=0.2, color='b', align='center', label = "normal")
    ax.bar(x+0.1, pos_dedup, width=0.2, color='r', align='center', label = "CDMT")
    for i in range(len(new)):
        new[i] = new[i][new[i].rfind('/') + 1:-7]

    #new.insert(0, "dummy")
    degree = 90
    ax.set_xticks(x)
    ax.set_xticklabels(new)
    ax.legend()
    print(new) 
    plt.xticks(rotation = degree)
    plt.title("duplicate rate in the tree in " + sys.argv[1])
    plt.ylabel("duplicate rate");
    plt.xlabel("versions")
    plt.savefig("3" + sys.argv[1] + ".png", format = 'png')
    

if __name__ == "__main__":
    main()
