<?xml version="1.0"?>
<!--
  XML stylesheet for formatting the navi web site
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!--================================== Document Root -->
  <xsl:template match="/page">
    <html>
      <head>
        <title><xsl:value-of select="@title"/></title>
        <style type="text/css" media="all"> @import url(style.css);</style>
      </head>
      <body>
        <xsl:choose>
          <xsl:when test="/page@index=true">
            <div class="majorTitle"><xsl:value-of select="/page@title"/></div>
          </xsl:when>
          <xsl:otherwise>
            <div class="heading">
              <div class="title"><xsl:value-of select="/page/heading"/></div>
              <div class="headingTabs">
                <xsl:apply-templates select="document('tabs.xml')//headingTab"/>
              </div>
            </div>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:apply-templates select="img"/>
      </body>
    </html>
  </xsl:template>

  <!--================================== Heading Tabs -->
  <xsl:template match="headingTab">
    <a class="headingTab">
      <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
      <xsl:value-of select="@name"/>
    </a>
  </xsl:template>

  <!--================================== Images -->
  <xsl:template match="img">
    <img class="centered">
      <xsl:attribute name="class">centered</xsl:attribute>
      <xsl:attribute name="src"><xsl:value-of select="@src"/></xsl:attribute>
      <xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
    </img>
  </xsl:template>

</xsl:stylesheet>
