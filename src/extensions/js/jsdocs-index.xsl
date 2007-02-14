<xsl:stylesheet version = "1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">
  <xsl:output method="html" encoding="iso-8859-1" indent="yes"/>

  <xsl:template match="//classes">
    <xsl:variable name="columns" select="3"/>
    <!--
    <xsl:variable name="sortedClasses">
      <xsl:for-each select="class">
        <xsl:sort select="@name"/>
        <xsl:copy-of select="."/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:variable name="classes" select="exslt:node-set($sortedClasses)"/>
    -->
    <html>
    <head>
    <title>KstScript Documentation</title>
    </head>
    <body>
    <h2>KstScript Classes:</h2>
    <table width="85%" border="0">
    <xsl:for-each select="class[position() mod $columns = 1 or $columns = 1]">
      <tr>
      <xsl:for-each select=". | following-sibling::class[position() &lt; $columns]">
        <td>
          <a href="{@name}.html"><xsl:value-of select="@name"/></a>
        </td>
      </xsl:for-each>
      </tr>
    </xsl:for-each>
    </table>

    <hr/>
    <h6>Copyright <xsl:text disable-output-escaping="yes">&amp;copy;</xsl:text> 2005-2006 <a href="http://www.utoronto.ca/">The University of Toronto</a><br/>
  Kst <a href="http://kst.kde.org/">http://kst.kde.org/</a></h6>
    </body>
    </html>
  </xsl:template>

</xsl:stylesheet>
