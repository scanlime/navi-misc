<?xml version="1.0" ?>
<!--
   XML stylesheet for formatting the navi-misc web site
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!--================================== Document Root -->

  <xsl:template match="/page">
    <html>
      <head>
        <title>navi-misc : <xsl:value-of select="@title"/></title>
        <style type="text/css" media="all"> @import url(style.css);</style>
      </head>
      <body>
        <div class="heading">
          <div class="siteName">navi-misc</div>
          <div class="title"><xsl:value-of select="@title"/></div>
          <div class="subtitle"><xsl:value-of select="@subtitle"/></div>
	  <div class="headingTabs">
	    <a class="headingTab" href="index.xml">projects</a>
	    <a class="headingTab" href="http://navi.picogui.org/svn/misc">repository</a>
	  </div>
        </div>

        <xsl:apply-templates select="introduction"/>

      </body>
    </html>
  </xsl:template>

  <!--================================== Introduction -->

  <xsl:template match="introduction">
    <span class="section">
  
    <div class="row">
      <xsl:attribute name="id">package--<xsl:value-of select="@name"/></xsl:attribute>
      <xsl:apply-templates select="require"/>
      <div class="item"><xsl:value-of select="@name"/></div>
      <xsl:apply-templates select="description"/>
      <span class="itemDetail">Versions</span>
      <div class="itemDetail">
        <xsl:apply-templates select="version">
          <xsl:sort select="@name"/>
        </xsl:apply-templates>
      </div>
    </div>
  </xsl:template>

</xsl:stylesheet>
