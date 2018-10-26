import os

def remove_if_exists(filename):
    if os.path.exists(filename):
        os.remove(filename)

def copy_if_not_exists(filename, path):
    if os.path.exists(filename) is False:
        os.symlink(path+"/"+filename, filename)
