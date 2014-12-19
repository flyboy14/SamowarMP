pkgname=samowar-beta-git
pkgver=29.e54c843
pkgrel=1
pkgdesc="Extremely lightweight music player, written in Qt5 (icons from buuf Deuce theme)"
arch=('any')
url="https://github.com/flyboy14/SamowarMP"
license=('GPL')
makedepends=('git' 'qt5-multimedia')
source=("$pkgname"::'git://github.com/flyboy14/SamowarMP#branch=buggy')
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
if [ ! -d ~/.config ]; then
  mkdir -p ~/.config
fi
if [ ! -d ~/.config/samowar ]; then
  mkdir -p ~/.config/samowar
fi
if [ ! -d /usr/share/samowar ]; then
  mkdir -p $pkgdir/usr/share/samowar 
fi
        cp -av ./icons $pkgdir/usr/share/samowar/
}
