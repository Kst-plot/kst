<xsl:stylesheet version = '1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method="html" encoding="iso-8859-1" indent="yes"/>

  <xsl:template match="//class">
    <xsl:variable name="thisClass" select="@name"/>
    <html>
    <head>
    <title><xsl:value-of select="@name"/> - KstScript Documentation</title>
    <style type="text/css" media="all">@import "./kstscript.css";</style>
    </head>
    <body class="mainPage">
    <div class="topHeader">
    <div class="header">Class:</div>
    <div class="headerMain"><xsl:value-of select="@name"/></div>
    </div>
    <br/>
    <br/>
    <br/>
    <div class="classInfo"><p><xsl:value-of select="//description" disable-output-escaping="yes"/></p><br/>
    
    <xsl:if test="count(inherits) &gt; 0">
      <xsl:for-each select="inherits">
        Inherits: <a class="link" href="{@name}.html"><xsl:value-of select="@name"/></a><br/>
      </xsl:for-each>
    </xsl:if>
    </div>
    <br/>
    
    <!-- List constructors -->
    <xsl:if test="count(constructor) &gt; 0">
      <span class="title">Constructors:</span>
      <ul>
      <xsl:for-each select="constructor">
        <a class="link" href="#constructor_{string(position())}"><xsl:value-of select="$thisClass"/></a>
        <xsl:text> ( </xsl:text>
        <xsl:call-template name="displayArgumentsInline">
          <xsl:with-param name="typed" select="0"/>
        </xsl:call-template>
        <xsl:text> )</xsl:text>
        <xsl:if test="@obsolete = 'true'">
          <xsl:text> [Obsolete]</xsl:text>
        </xsl:if>
        <br/>
      </xsl:for-each>
      </ul>
    </xsl:if>

    <!-- List all the methods -->
    <xsl:if test="count(method) &gt; 0">
      <span class="title">Methods:</span>
      <ul>
      <xsl:for-each select="method">
        <xsl:choose>
          <xsl:when test="@obsolete = 'true'">
            <xsl:value-of select="@name"/>
          </xsl:when>
          <xsl:otherwise>
            <a class="link" href="#method_{@name}"><xsl:value-of select="@name"/></a>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text> ( </xsl:text>
        <xsl:call-template name="displayArgumentsInline">
          <xsl:with-param name="typed" select="0"/>
        </xsl:call-template>
        <xsl:text> )</xsl:text>
        <xsl:if test="@obsolete = 'true'">
          <xsl:text> [Obsolete]</xsl:text>
        </xsl:if>
        <br/>
      </xsl:for-each>
      </ul>
    </xsl:if>

    <!-- List all the properties -->
    <xsl:if test="count(property) &gt; 0">
      <span class="title">Properties:</span>
      <ul>
      <xsl:for-each select="property">
        <xsl:choose>
          <xsl:when test="@obsolete = 'true'">
            <xsl:value-of select="@name"/><xsl:text> [Obsolete]</xsl:text><br/>
          </xsl:when>
          <xsl:otherwise>
            <a class="link" href="#property_{@name}"><xsl:value-of select="@name"/></a><br/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
      </ul>
    </xsl:if>

    <xsl:for-each select="collection">
      Collection class: <a class="link" href="{@name}.html"><xsl:value-of select="@name"/></a><br/>
    </xsl:for-each> 
    <hr/><br/>

    <!-- Display details of constructors -->
    <xsl:for-each select="constructor">
      <span class="subTitle">
      <a name="constructor_{string(position())}"/>
      <b><xsl:value-of select="$thisClass"/><xsl:text> ( </xsl:text>
      <xsl:call-template name="displayArgumentsInline"/>
      <xsl:text> )</xsl:text>
      </b>
      </span>
      <br/>

      <ul>
      <div class="parameters">
      <xsl:call-template name="displayArgumentsFull"/>
      </div>
      <p><xsl:value-of select="description" disable-output-escaping="yes"/></p>
      <xsl:call-template name="displayExceptions"/>
      </ul>

      <br/><br/>
    </xsl:for-each>

    <!-- Display details of methods -->
    <xsl:for-each select="method">
      <span class="subTitle">
      <a name="method_{@name}"/>
      <b>

      <!-- return type -->
      <xsl:call-template name="displayReturnType"/>

      <xsl:value-of select="@name"/>
      <xsl:text> ( </xsl:text>
      <xsl:call-template name="displayArgumentsInline"/>
      <xsl:text> )</xsl:text>
      </b>
      <xsl:if test="@obsolete = 'true'">
        <xsl:text> [Obsolete]</xsl:text>
      </xsl:if>
      </span>
      <br/>

      <ul>
      <div class="parameters">
      <xsl:call-template name="displayArgumentsFull"/>
      </div>
      <p><xsl:value-of select="description" disable-output-escaping="yes"/></p>
      <xsl:call-template name="displayExceptions"/>
      </ul>

      <br/><br/>
    </xsl:for-each>

    <!-- Display details of properties -->
    <xsl:for-each select="property">
      <span class="subTitle">
      <a name="property_{@name}"/>
      <b>
      <xsl:call-template name="displayReturnType"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:if test="@readonly = 'true'">
        <xsl:text> [Read-Only]</xsl:text>
      </xsl:if>
      <xsl:if test="@obsolete = 'true'">
        <xsl:text> [Obsolete]</xsl:text>
      </xsl:if>
      </b></span><br/>
      <ul>
      <p><xsl:value-of select="description" disable-output-escaping="yes"/></p>
      </ul>
      <br/><br/>
    </xsl:for-each>

    <br/>
    <a class="link" href="classindex.html">&lt;-- Back To Index</a><xsl:text disable-output-escaping="yes">&amp;nbsp;&amp;nbsp;&amp;nbsp;</xsl:text><a class="link" href="index.html">[Overview]</a>
    <hr/>
    <h6>Copyright <xsl:text disable-output-escaping="yes">&amp;copy;</xsl:text> 2005-2006 <a class="link" href="http://www.utoronto.ca/">The University of Toronto</a><br/>
  Kst <a class="link" href="http://kst.kde.org/">http://kst.kde.org/</a></h6>
    </body>
    </html>
  </xsl:template>

  <xsl:template name="displayReturnType">
    <xsl:choose>
      <xsl:when test="@type and not(@type = '')">
        <xsl:choose>
          <xsl:when test="@type = 'number' or @type = 'string' or @type = 'boolean' or @type = 'date'">
            <xsl:value-of select="@type"/>
          </xsl:when>
          <xsl:otherwise>
            <a class="link" href="{@type}.html"><xsl:value-of select="@type"/></a>
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
            <xsl:when test="@type = 'number' or @type = 'string' or @type = 'boolean' or @type = 'date'">
              <xsl:value-of select="@type"/>
            </xsl:when>
            <xsl:otherwise>
              <a class="link" href="{@type}.html"><xsl:value-of select="@type"/></a>
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
  
  <xsl:template name="displayArgumentsFull">
    <xsl:for-each select="argument">
      <xsl:choose>
        <xsl:when test="@type = 'number' or @type = 'string' or @type = 'boolean' or @type = 'date'">
          <xsl:value-of select="@type"/>
        </xsl:when>
        <xsl:otherwise>
          <a class="link" href="{@type}.html"><xsl:value-of select="@type"/></a>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text> </xsl:text>
      <b><xsl:value-of select="@name"/></b>
      <xsl:if test="description">
        <xsl:text> - </xsl:text>
        <xsl:value-of select="description" disable-output-escaping="yes"/>
      </xsl:if>
      <xsl:if test="@optional = 'true'">
        <xsl:text> [OPTIONAL]</xsl:text>
      </xsl:if>
      <br/>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="displayExceptions">
    <xsl:for-each select="exception">
      Throws: <b><xsl:value-of select="@name"/></b>
      <xsl:if test="description">
        <xsl:text> - </xsl:text>
        <xsl:value-of select="description" disable-output-escaping="yes"/>
      </xsl:if>
      <br/>
    </xsl:for-each>
  </xsl:template>

</xsl:stylesheet>
