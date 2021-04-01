import imageio
import os
import glob

files = glob.glob('./img/*')

images = []
filenames = 'img'

n = len(glob.glob('./movie/*'))
name = 'movie'+str(n+1)

for i in range(len(files)):
    images.append(imageio.imread('img/'+str(i)+'.png'))
imageio.mimsave('./movie/%s.gif' %name, images)

#erase images
files = glob.glob('./img/*')
for f in files:
    os.remove(f)