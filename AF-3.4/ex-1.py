from time import sleep
from random import randint
from threading import Thread, Lock, Condition

def produtor():
  global buffer, i_prod
  while True:
    sleep(randint(0, 2))  # fica um tempo produzindo...
    with lock:
      if i_prod >= n_itens:
        break
      item = 'item ' + str(i_prod)
      while len(buffer) == tam_buffer:
        print('>>> Buffer cheio. Produtor ira aguardar.')
        lugar_no_buffer.wait()    # aguarda que haja lugar no buffer
      buffer.append(item)
      print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
      i_prod += 1
      item_no_buffer.notify_all()

def consumidor():
  global buffer, i_cons
  while True:
    with lock:
      if i_cons >= n_itens:
        break
      while len(buffer) == 0:
        print('>>> Buffer vazio. Consumidor ira aguardar.')
        item_no_buffer.wait()   # aguarda que haja um item para consumir 
      item = buffer.pop(0)
      print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
      i_cons += 1
      lugar_no_buffer.notify_all()
    sleep(randint(0,2))         # fica um tempo consumindo...

buffer = []
tam_buffer = 5
i_cons = 0
i_prod = 0
n_itens = 10
lock = Lock()
lugar_no_buffer = Condition(lock)
item_no_buffer = Condition(lock)

produtores = [None]*2
consumidores = [None]*2
for i in range(1):
  produtores[i] = Thread(target=produtor)
  consumidores[i] = Thread(target=consumidor)

for i in range(1):
  produtores[i].start()
  consumidores[i].start()

for i in range(1):
  produtores[i].join()
  consumidores[i].join()