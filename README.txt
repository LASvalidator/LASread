LASread
=======

A light-weight open source (LGPL) package for reading LAS/LAZ files. This code is a completely re-engineered subset of the popular LASlib library of LAStools. The code seamlessly integrate the LASzip compression library so that files in the LAS format can also be verified directly in their lossless compressed twin representation, the LAZ format. It had been significantly simplified to better suit the requirements of the request for proposals of the ASPRS for the LAS Validation Suite (LVS). However, during contract negotiations the ASPRS explicitly requested that the LVS was *not* to be developped in an open process. It is probably for the best that negotiations failed because the "LASvalidator" is now open source.

The great benefit of the immediate release of the LASvalidator in open source is that it has already and is still being field-tested and crowd-improved. The transparent development process has also allowed others to contribute bug fixes and voice implementation concerns early.
