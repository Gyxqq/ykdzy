import json

f=open('playerconfig.json')
player=json.load(f)
f.close()
f=open('apple.png','rb')
apple=f.read()
f.close()
for i in player["run_left"]:
    f=open(i,'wb')
    f.write(apple)
    f.close()
for i in player["run_right"]:
    f=open(i,'wb')
    f.write(apple)
    f.close()
for i in player["jump"]:
    f=open(i,'wb')
    f.write(apple)
    f.close()
    