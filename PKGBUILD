# Maintainer: John Lindgren <john@jlindgren.net>

pkgname=volumeicon-qt
pkgver=0.6
pkgrel=1
pkgdesc='Volume control for the system tray (Qt port)'
arch=(x86_64)
license=(GPL3)
depends=(glib2 qt5-base alsa-lib libnotify)
conflicts=('volumeicon')
provides=('volumeicon')

build() {
  cd ..
  ./autogen.sh
  ./configure --prefix=/usr
  make
}

package() {
  cd ..
  make DESTDIR="$pkgdir" install
}
