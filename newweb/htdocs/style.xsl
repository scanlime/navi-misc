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
        <div class="heading">
          <div class="title"><xsl:value-of select="@title"/></div>
          <div class="headingTabs">
            <xsl:apply-templates select="document('tabs.xml')//headingTab"/>
          </div>
        </div>
        <table class="columns">
          <!--====== Navigation -->
          <td class="left">
          </td>

          <!--====== Navigation -->
          <td class="main">
            <xsl:apply-templates select="section"/>
          </td>
        </table>

        <div class="footer">
          <a href="/">
            <img src="http://navi/images/web/navi64.png" width="64" height="39" alt="Navi"/>
          </a>
        </div>
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

  <!--================================== Content Sections -->
  <xsl:template match="section">
    <span class="section"><xsl:value-of select="@title"/></span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        <xsl:apply-templates/>
      </div>
    </div>
  </xsl:template>

</xsl:stylesheet>
