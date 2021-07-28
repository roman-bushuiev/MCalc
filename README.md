# MCalc

MCalc is a command-line calculator suitable for basic linear algebra computations. It allows to store matrices and scalars in variables and to perform on them such operations like multiplication, subtraction, exponentiation, finding matrix determinant, Gaussian elimination, matrix transposition, and others. Since MCalc treats sparse and dense matrices in different ways, it provides high-speed operations even for large inputs.

Example of the program run:
```
--Input-->:
2+2*2
<--Output--:
6
--Input-->:
s = {[1,1,1],[1,1,1],[1,1,1]}
--Input-->:
s
<--Output--:
Name: <s> ; Type: <Dense> ; Rows: <3> ; Columns: <3> ; Zeros: <0>
/          1          1          1         \
|          1          1          1         |
\          1          1          1         /
--Input-->:
rank(s) == 1
<--Output--:
1
--Input-->:
k = {[2,6,7],[17,3,37],[77,2,17]}
--Input-->:
ge(k)
<--Output--:
Name: <> ; Type: <Dense> ; Rows: <3> ; Columns: <3> ; Zeros: <3>
/         77          2         17         \
|          0   5.948052   6.558442         |
\          0          0   30.42576         /
--Input-->:
F = {[5,6],[4,6],[7,10]}
--Input-->:
F*F
<--Output--:
Multiplied matrixes have inconpatible sizes.
--Input-->:
T = F*transpose(F)
--Input-->:
T
<--Output--:
Name: <T> ; Type: <Dense> ; Rows: <3> ; Columns: <3> ; Zeros: <0>
/         61         56         95         \
|         56         52         88         |
\         95         88        149         /
--Input-->:
0.5*split(T,1,1,2,2)
<--Output--:
Name: <> ; Type: <Dense> ; Rows: <2> ; Columns: <2> ; Zeros: <0>
/         26         44         \
\         44       74.5         /
```
