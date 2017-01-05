Name:           kde-plasma-mawakit
Version:        0.3.0
Release:        2%{?dist}
Summary:        Mawakit is a prayer times plasmoid that plays the Islamic Adhan when it is time for prayer.

Group:          User Interface/Desktops
License:        GPLv2+
URL:            http://git.ojuba.org/cgit/plasma-widget-mawakit/
%define         upstream_name plasma-widget-mawakit
%define         upstream_version %{version}
#%%define         upstream_version master
Source0:        http://git.ojuba.org/cgit/%{upstream_name}/snapshot/%{upstream_name}-%{upstream_version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  kdebase-workspace-devel >= 4.2.0
BuildRequires:  gettext, cmake, make, gcc, kdelibs-devel

%description
Mawakit is a prayer times plasmoid that plays the Islamic Adhan when it is time for prayer.
Designed to be simple and elegant, Mawakit shows a countdown until the next prayer in a nice tooltip.

%prep
%setup -qn %{upstream_name}

%build
mkdir -p %{_target_platform}
pushd %{_target_platform}
%{cmake_kde4} ..
popd

make VERBOSE=1 %{?_smp_mflags} -C %{_target_platform}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot} -C %{_target_platform}


%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc COPYING README
%{_kde4_libdir}/kde4/plasma_applet_mawakit.so
%{_kde4_datadir}/kde4/services/plasma_applet_mawakit.desktop
%{_kde4_datadir}/kde4/apps/mawakit/*
%{_datadir}/locale/*/*/*.mo
%{_datadir}/icons/oxygen/*/apps/kaba.*

%changelog
* Wed Mar 9 2011 Ahmed atalla <amineroukh@gmail.com> 0.1.0-3
- Rename from mawakit to plasma_applet_mawakit
* Wed Aug 11 2010 Ahmed atalla <a.atalla@linuxac.org> 0.1.0-2
- rename to kde-plasma-mawakit
* Wed Aug 11 2010 Ahmed atalla <a.atalla@linuxac.org> 0.1.0-1
- Initial package