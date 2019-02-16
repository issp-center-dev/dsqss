from setuptools import setup

setup(name='dsqss',
      version='2.0.0',
      description='DSQSS input files generator',
      url='https://github.com/issp-center-dev/dsqss',
      author='DSQSS developers',
      author_email='dsqss-dev@issp.u-tokyo.ac.jp',
      license='GPLv3',
      packages=['dsqss'],
      python_requires='>=2.7',
      install_requires=['numpy', 'scipy', 'toml'],
      entry_points={
          'console_scripts':[
              'dla_hamgen = dsqss.std_model:main',
              'dla_latgen = dsqss.std_lattice:main',
              'dla_sfgen = dsqss.wavevector:main',
              'dla_alg = dsqss.dla_alg:main',
              'dla_pre = dsqss.parameter:main',
              ]
          },
      zip_safe=False,
      )
