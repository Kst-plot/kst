<xsl:stylesheet version = "1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="srcdir"/>
  <xsl:output method="text" encoding="iso-8859-1"/> 

  <xsl:template match="//classes">
  <xsl:text>#!/bin/sh
#

</xsl:text>
    <xsl:for-each select="class">
<xsl:value-of select="$srcdir"/>/jsdocs2xml.pl <xsl:value-of select="$srcdir"/>/<xsl:value-of select="@file"/>.h
</xsl:for-each>
    <xsl:text>
</xsl:text>
xsltproc <xsl:value-of select="$srcdir"/>/jsdocs-index.xsl <xsl:value-of select="$srcdir"/>/classindex.xml &gt;classindex.html
    <xsl:for-each select="class">
[ ! -e <xsl:value-of select="@file"/>.xml ] ||
xsltproc <xsl:value-of select="$srcdir"/>/jsdocs-class.xsl <xsl:value-of select="@file"/>.xml &gt; <xsl:value-of select="@name"/>.html</xsl:for-each>
    <xsl:text>
</xsl:text>
  </xsl:template>

</xsl:stylesheet>
