<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" />

<xsl:template match="/doxygen">
<classes>
  <xsl:for-each select="compound[@kind='class']">
    <class><xsl:value-of select="name/text()" /></class>
  </xsl:for-each>
</classes>
</xsl:template>
</xsl:stylesheet>
