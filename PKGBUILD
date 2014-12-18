pkgname=samowar-beta-git
pkgver=2.3.19b
pkgrel=1
pkgdesc="Extremely lightweight music player, written in Qt5 (icons from buuf Deuce theme)"
arch=('any')
url="https://github.com/flyboy14/SamowarMP"
license=('GPL')
makedepends=('git' 'qt5-multimedia')
source=("$pkgname"::'git://github.com/flyboy14/SamowarMP#branch=playlist')
md5sums=( 'SKIP' )

pkgver() {
  cd ${pkgname}
  echo $(git rev-list --count HEAD).$(git rev-parse --short HEAD)
}



build() {
cd "$srcdir/$pkgname"
qmake
make
}

package() {
  cd $srcdir/$pkgname
  install -Dm755 ./kurs $pkgdir/usr/bin/samowar
}
