LASread
=======

A light-weight open source (LGPL) package for reading LAS/LAZ files. This code is a completely re-engineered subset of the popular LASlib library of LAStools. It has been significantly simplified to better suit the requirements of the request for proposals of the ASPRS for the LAS Validation Suite (LVS). One advantage of the immediate release of open source is that it can immediately be field-tested and crowd-improved and allow others to to contribute bug fixes and implementation concerns early. Furthermore this code will seamlessly integrate the LASzip compression library so that files in the LAS format – should this be so desired – could also be verified directly in their lossless compressed twin representation, the LAZ format.

Temporary Restriction: This code is currently being used by rapidlasso GmbH for bidding on the request for proposals (RFP) that was issued by the ASPRS for the implementation of the LAS Validation Suite (LVS). It is not allowed to make use of any parts of this code to submit a competing bid.
