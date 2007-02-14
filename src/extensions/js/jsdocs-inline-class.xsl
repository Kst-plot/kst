<xsl:stylesheet version = '1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method="text" encoding="iso-8859-1"/>

  <xsl:template match="//class">
    <xsl:variable name="thisClass" select="@name"/>
    Class: <xsl:value-of select="@name"/>
    <xsl:value-of select="//description" disable-output-escaping="yes"/>
    
      <xsl:text>
      </xsl:text>

    <!-- List constructors -->
    <xsl:for-each select="constructor">
      <xsl:value-of select="$thisClass"/>(<xsl:call-template name="displayArgumentsInline">
        <xsl:with-param name="typed" select="0"/>
      </xsl:call-template><xsl:text>)
      </xsl:text>
    </xsl:for-each>

      <xsl:text>
      </xsl:text>

    <!-- List all the methods -->
    <xsl:for-each select="method">
      <xsl:call-template name="displayReturnType"/> <xsl:value-of select="@name"/>(<xsl:call-template name="displayArgumentsInline">
        <xsl:with-param name="typed" select="0"/>
      </xsl:call-template><xsl:text>)
      </xsl:text>
    </xsl:for-each>

      <xsl:text>
      </xsl:text>

    <!-- List all the properties -->
    <xsl:for-each select="property">
      <xsl:call-template name="displayReturnType"/> <xsl:value-of select="@name"/>
      <xsl:if test="@readonly = 'true'">
        <xsl:text> [Read-Only]</xsl:text>
      </xsl:if>
      <xsl:text>
      </xsl:text>
    </xsl:for-each>

    <xsl:for-each select="collection">
      Collection class: <xsl:value-of select="@name"/>
    </xsl:for-each> 

    <xsl:text>
</xsl:text>

  </xsl:template>

  <xsl:template name="displayReturnType">
    <xsl:choose>
      <xsl:when test="@type and not(@type = '')">
        <xsl:choose>
          <xsl:when test="@type = 'number' or @type = 'string' or @type = 'boolean'">
            <xsl:value-of select="@type"/>
          </xsl:when>
          <xsl:otherwise>
            <a href="{@type}.html"><xsl:value-of select="@type"/></a>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>void </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="displayArgumentsInline">
    <xsl:param name="typed" select="1"/>
    <xsl:param name="arg" select="1"/>
    <xsl:if test="$arg &lt; count(argument) + 1">
      <xsl:for-each select="argument[$arg]">
        <xsl:if test="@optional = 'true'"> [</xsl:if>
        <xsl:if test="$arg &gt; 1">, </xsl:if>
        <xsl:if test="$typed = 1">
          <xsl:choose>
            <xsl:when test="@type = 'number' or @type = 'string' or @type = 'boolean'">
              <xsl:value-of select="@type"/>
            </xsl:when>
            <xsl:otherwise>
              <a href="{@type}.html"><xsl:value-of select="@type"/></a>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:value-of select="@name"/>
	<xsl:for-each select="..">
          <xsl:call-template name="displayArgumentsInline">
            <xsl:with-param name="typed" select="$typed"/>
            <xsl:with-param name="arg" select="$arg + 1"/>
          </xsl:call-template>
	</xsl:for-each>
        <xsl:if test="@optional = 'true'">]</xsl:if>
      </xsl:for-each>
    </xsl:if>
  </xsl:template>
  
</xsl:stylesheet>
