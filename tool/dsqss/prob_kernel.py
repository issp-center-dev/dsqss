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

def reversible_suwa_todo(weights):
    '''
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    under the detailed balance condition
    '''
    N = len(weights)
    ret = np.zeros((N,N))
    indices = np.argsort(weights, kind='mergesort')[::-1]
    weights = np.array(weights)
    W = np.zeros([N,N])
    for i in range(N):
        I = indices[i]
        W[I,I] = weights[I]

    S3 = sum(weights[indices[2:]])
    wdiff = weights[indices[0]] - weights[indices[1]]
    if wdiff >= S3:
        for i in range(1,N):
            I = indices[i]
            rst_swap(indices[0], I, weights[I], W)
    else:
        w3 = wdiff/S3
        for i in range(2,N):
            I = indices[i]
            v = w3 * weights[I]
            rst_swap(0, I, v, W)
        for j in range(N-1, 0, -1):
            J = indices[j]
            v = W[J, J] / j
            for k in range(j-1, -1, -1):
                rst_swap(J, indices[k], v, W)
    for i in range(N):
        for j in range(N):
            W[i,j] /= weights[i]
    return W

def rst_swap(i,j,w,W):
    W[i,i] -= w
    W[i,j] += w
    W[j,i] += w
    W[j,j] -= w
