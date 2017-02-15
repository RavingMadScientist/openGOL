# -*- coding: utf-8 -*-
"""
Created on Tue Jan 24 15:22:23 2017

@author: legitz7
"""

f=open("blub.txt", 'w')
for i in range(11):
    for j in range(11):
        for k in range(11):
            if (j==k and i==0 and j!=8 and j!=6):
                f.write('1;')
            elif( (k%2 == 0) and j==0 and i==0):
                f.write('1;')
            elif( (k%3 == 0) and (j==0 or j==1) and i==1):
                f.write('1;')
            else:
                f.write('0;')
f.close()
            
