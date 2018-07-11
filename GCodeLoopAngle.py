import os


#Input parameters
name = 'FuroMaiorRodaSmallV1'

appendName = "Recursively"

angle = 72

total = int(360/angle)

inpath = "~/Documents/SMALL/Roda/"

outpath = "~/Documents/SMALL/Roda/Recursive"

#Loading sherline archive

command = "sshpass -p \"sherline\" scp sherline@192.168.0.26:"+inpath+name+'.ngc'+" "+"./"
os.system(command)

#Opening input archive and reading lines
input = open(name+'.ngc', 'r')

pal = input.readlines()

#Formatting output
pal = pal[7:-2]

pal += ['G0 A'+ str(angle)+'\n']
pal += ['G92 A0\n']

#Debug
print(pal)

pal2 = []

for i in range(total):
     pal2+=pal

#Writting output archive
output = open(name+appendName+'.ngc', 'w')
output.write(''.join(['%\n']+pal2+['M30\n']+['%\n']))

input.close()
output.close()

#Sending output to sherline
command = "sshpass -p \"sherline\" scp ./"+name+appendName+'.ngc'+" "+"sherline@192.168.0.26:"+outpath
os.system(command)

#Deleting temporary files in the pc
os.system("find . -name \"*.ngc\" -type f -delete")
