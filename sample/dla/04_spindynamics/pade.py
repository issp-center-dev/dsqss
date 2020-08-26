import re
import numpy as np

f = open("ck.dat")
for line in f:
    line_array = re.split(" +", line)
    if (line_array[0] == "P") and (line_array[1] == "L"):
        L = int(int(line_array[3]) / 2)
        break
print("# Lattice Size: %i" % (2 * L))

for line in f:
    line_array = re.split(" +", line)
    if (line_array[0] == "P") and (line_array[1] == "BETA"):
        BETA = float(line_array[3])
        break
print("# Inverse temperature: %f" % (BETA))

for line in f:
    line_array = re.split(" +", line)
    if (line_array[0] == "P") and (line_array[1] == "NTAU"):
        NTAU = int(line_array[3])
        break
print("# Number of division in tau direction: %i" % (NTAU))

ck = np.zeros((L + 1, NTAU), dtype=np.complex)
for line in f:
    line_array = re.split(" +", line)
    if line_array[0] == "R":
        str = line_array[1]
        str = str[1:]
        str_array = str.split("t")
        ck[int(str_array[0])][int(str_array[1])] = complex(
            float(line_array[3]), float(line_array[4])
        )
print("# Date is read.")

dt = BETA / NTAU
t = np.arange(0, NTAU * dt, dt)
cktmp = np.zeros(NTAU, dtype=np.complex)
for ikloop in range(2 * L - 1):
    ik = abs(ikloop - L + 1)
    for j in range(NTAU):
        cktmp[j] = ck[ik][j]
    ckw = np.fft.fft(cktmp) / (float(NTAU) / 2.0)
    ndata = int(NTAU / 4) - 1
    a_pre = []
    for k in range(ndata):
        a_pre.append(complex(ckw[k + 1].real, 0.0))
    z_n = np.array([(2 * im + 2) * np.pi / BETA for im in range(ndata)])
    a = [0.0] * ndata
    a[0] = a_pre[0]
    for p in range(1, ndata):
        for i in range(p, ndata):
            a_pre[i] = (a_pre[p - 1] / a_pre[i] - 1.0) / (1j * (z_n[i] - z_n[p - 1]))
            a[p] = a_pre[p]
    omega_min = 0.0
    omega_max = 3.0
    omega_nd = 200
    omega_h = (omega_max - omega_min) / omega_nd
    for i in range(omega_nd + 1):
        omega = omega_min + omega_h * i + 1j * 0.01
        tmp = 1.0 + 1j * 0.0
        for p in range(ndata - 1, 0, -1):
            tmp = 1.0 + a[p] * (omega - 1j * z_n[p - 1]) / tmp
        tmp = a[0] / tmp
        print(float(ikloop + 1) / L - 1.0, omega.real, tmp.real, tmp.imag)
    print("")
