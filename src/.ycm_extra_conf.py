import os
import ycm_core

flags = [
'-Wall',
'-Wextra',
'-Werror',
'-fexceptions',
'-std=c++14',
'-x',
'c++',
'-I', './',
'-I', '../thirdparty/Box2D'
'-I', '../thirdparty/SFML'
'-I', '../thirdparty/enet'
'-I', '../thirdparty/cereal'
]

def FlagsForFile( filename, **kwargs ):
  return {
    #'flags': final_flags,
    'flags':flags,
    'do_cache': True
  }
