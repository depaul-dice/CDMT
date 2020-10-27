import matplotlib.pyplot as plt

def parse_read(prefix, image):
    path = prefix + image + '.txt'
    oldpaths, newpaths, leaves, nc, dr = ([] for i in range(5))
    with open(path, 'r') as f:
        while True:
            line = f.readline().rstrip(' \n')
            if line == "":
                break
            image_, oldpath, newpath, leaf, nodes_compared, dedup_ratio = line.split(',') 
            oldpaths.append(oldpath)
            newpaths.append(newpath)
            leaves.append(leaf)
            nc.append(nodes_compared)
            dr.append(dr)

    return oldpaths, newpaths, leaves, nc, dr

def get_real_dr(image: str, tmp_newpath: list):
    prefix = "/home/yutan/cdmt/results/experiment2/"
    dr = list()
    filenames = list()
    with open(prefix + image + '.txt', 'r') as f:
        f.readline() # throwing away the oldest one
        for path in tmp_newpath:
            line = f.readline().rstrip('\n ')
            filename, dedup_rate, dedup_size, gz_rate, gz_size = line.split(':')
            if filename + '.tar.bk' != path[path.rfind('/') + 1:]:
                print(filename)
                print(path[path.rfind('/') + 1:])
                raise Exception('wrong format')
            
            dr.append(1 - float(dedup_rate))
            filenames.append(filename)

        filename, dedup_rate, dedup_size, gz_rate, gz_size = f.readline().rstrip('\n ').split(':')
        assert(filename == 'global')
    return dr, filenames

def graph(imgs, leaves, compared, dedup_ratio):
    dictionary = dict()
    for i in range(len(imgs)):
        if imgs[i] in dictionary:
            dictionary[imgs[i]].append([leaves[i], compared[i], dedup_ratio[i]])
        else:
            dictionary[imgs[i]] = list()
            dictionary[imgs[i]].append([leaves[i], compared[i], dedup_ratio[i]])

    # leaves, compared, dedup_ratio = ([] for i in range(3))
    fig, ax = plt.subplots(figsize = (12, 7))
    plt.gcf().subplots_adjust(bottom = 0.25)
    fontdict = {'fontsize': 12, 'fontweight': 'bold'}
    legend_properties = {'weight': 'bold'}
    for image, value in dictionary.items():
        ls, cs, ds = ([] for i in range(3))
        for each in value:
            ls.append(int(each[0]))
            cs.append(int(each[1]))
            ds.append(float(each[2]))
        ax.scatter(ds, cs, s = 100, label = image.capitalize())
        ax.legend(fontsize = 12, prop = legend_properties)
    plt.xlabel('Deduplication Ratio')
    plt.xlim((0, 1))
    plt.savefig("../pics/experiment4.png", format = "png")


def main():
    images = ['deepmind', 'django', 'mysql', 'nginx', 'postgres', 'pytorch', 'golang', 'node', 'tomcat', 'tensorflow', 'r-base', 'python', 'redis', 'rails']
    
    imgs, filenames, leaves, nodes_compared, dedup_ratio = ([] for i in range(5)) 
    prefix = "/home/yutan/cdmt/results/experiment4/"

    for image in images:
        tmp_oldpath, tmp_newpath, tmp_leave, tmp_nc, tmp_dr = parse_read(prefix, image)

        for i in range(len(tmp_oldpath)):
            imgs.append(image)
        leaves.extend(tmp_leave)
        nodes_compared.extend(tmp_nc)
        # print(new_paths)
        tmp_dr, tmp_fn = get_real_dr(image, tmp_newpath)
        filenames.extend(tmp_fn)
        dedup_ratio.extend(tmp_dr)
    
    graph(imgs, leaves, nodes_compared, dedup_ratio)
    
if __name__ == "__main__":
    main()
