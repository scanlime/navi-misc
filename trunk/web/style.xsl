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

	<div class="navbar">
	    <div class="section">
              <div class="sectionTop">Projects</div>
	      <div class="row">
	        boing
              </div>
            </div>
	</div>

	<div class="content">
          <xsl:apply-templates select="introduction"/>
	</div>

      </body>
    </html>
  </xsl:template>

  <!--================================== Introduction -->

  <xsl:template match="introduction">
    <span class="section">Introduction</span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        boing
      </div>
    </div>
  </xsl:template>

</xsl:stylesheet>
