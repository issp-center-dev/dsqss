from copy import deepcopy
import numpy as np

def heat_bath(weights):
    '''
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the heat bath algorithm
    '''
    ws = np.array(weights)
    ws /= ws.sum()
    return np.array([ws for i in range(len(ws))])

def metropolice(weights):
    '''
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Metropolice algorithm
    '''
    N = len(weights)
    n = np.count_nonzero(weights)

    ret = np.zeros((N,N))
    for i in range(N):
        if weights[i] == 0.0:
            continue
        acc = 0.0
        for j in range(N):
            if i==j:
                continue
            p = min(weights[j]/weights[i], 1.0)/(n-1)
            ret[i,j] = p
            acc += p
        ret[i,i] = 1.0-acc
    return ret

def suwa_todo(weights):
    '''
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    '''
    N = len(weights)
    ret = np.zeros((N,N))
    indices = np.argsort(weights, kind='mergesort')[::-1]
    target = deepcopy(weights)
    for i in range(N):
        s = weights[indices[i]]
        j = 1
        while target[indices[j]] == 0.0:
            j = (j+1)%N
        while s > 0.0:
            p = min(s, target[indices[j]])
            s -= p
            target[indices[j]] -= p
            ret[indices[i], indices[j]] = p/weights[indices[i]]
            if target[indices[j]] <= 0.0:
                if j == i:
                    break
                j = (j+1)%N
    return ret

### breaking ergodicity???
### e.g.) when w = [5,4,3,2,1],
###       it returns W[4,:] = [0,0,0,0,1]
#  def detailed_suwa_todo(weights):
#      '''
#      return an array W,
#      where W[i,j] is the probability of a transition i=>j
#      calculated by the Suwa-Todo algorithm
#      under the detailed balance condition
#      '''
#      N = len(weights)
#      ret = np.zeros((N,N))
#      indices = np.argsort(weights, kind='mergesort')[::-1]
#      target = deepcopy(weights)
#      for i in range(N):
#          print(i, target)
#          s = target[indices[i]]
#          if s <= 0.0:
#              continue
#          j = (i+1)%N
#          while target[indices[j]] == 0.0:
#              j = (j+1)%N
#          while s > 0.0:
#              p = min(s, target[indices[j]])
#              print(i,j,s,target[indices[j]],p)
#              s -= p
#              target[indices[j]] -= p
#              target[indices[i]] -= p
#              ret[indices[i], indices[j]] = p/weights[indices[i]]
#              ret[indices[j], indices[i]] = p/weights[indices[j]]
#              if target[indices[j]] <= 0.0:
#                  if j == i:
#                      break
#                  j = (j+1)%N
#      return ret

