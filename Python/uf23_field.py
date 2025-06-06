import pyvista as pv
import numpy as np
from uf23 import UF23

gmf = UF23("base")

# field at one position ...
B = gmf.get_field(1, 3, 2)
print('field at (1,3,2) is', B)

# ... or at many
points = np.array([
    [0, 0, 0],
    [1, 3, 2],
    [-8.2, 0, 0]
])
B = gmf.get_field_many(points)
print('field at\n', points, '\nis\n', B)
