#
# This setup performs three main tasks
#
# 1) Install all the necessary development software
# 2) Create a user called "student"
# 3) Link the student's home directory to /vagrant
#
class {'update-repos': }

class dev-environment {
  package {'clang':
    ensure => latest,
  }
  package {'gcc':
    ensure => latest,
  }
  package {'gcc-doc':
    ensure => latest,
    require => Class['update-repos'],
  }
  package {'gdb':
    ensure => latest,
  }
}
class {'dev-environment': }

Class['update-repos'] -> Class['dev-environment']

