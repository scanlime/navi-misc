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
          <div class="title"><xsl:value-of select="@title"/></div>
          <div class="subtitle"><xsl:value-of select="@subtitle"/></div>
	  <div class="headingTabs">
	    <a class="headingTab" href="index.xml">navi-misc</a>
	  </div>
        </div>

        <xsl:apply-templates select="packages"/>
        <xsl:apply-templates select="sites"/>

      </body>
    </html>
  </xsl:template>

  <!--================================== Packages -->

  <xsl:template match="packages">
    <span class="section">Packages</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:apply-templates select="package">
        <xsl:sort select="@name"/>
      </xsl:apply-templates>
    </div>
  </xsl:template>

  <xsl:template match="package">
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

  <xsl:template match="require">
    <span class="itemTag">Required</span>
  </xsl:template>

  <xsl:template match="version">
    <div class="row">
      <xsl:value-of select="../@name"/>-<xsl:value-of select="@name"/>
      <ul>
        <xsl:apply-templates select="a">
          <xsl:sort select="@href"/>
        </xsl:apply-templates>
      </ul>
    </div>
  </xsl:template>

  <!--================================== Download sites -->

  <xsl:template match="sites">
    <span class="section">Download Sites</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:apply-templates select="site">
        <xsl:sort select="@name"/>
      </xsl:apply-templates>
    </div>
  </xsl:template>

  <xsl:template match="site">
    <div class="row">
      <xsl:attribute name="id">site--<xsl:value-of select="@name"/></xsl:attribute>
      <div class="item"><xsl:value-of select="@name"/></div>
      <span class="itemDetail">Mirrors</span>
      <div class="itemDetail">
        <ul>
          <xsl:apply-templates select="a">
            <xsl:sort select="@href"/>
          </xsl:apply-templates>
        </ul>
      </div>
    </div>
  </xsl:template>

  <!--================================== Descriptions -->

  <xsl:template match="description">
    <span class="itemDetail">Description</span>
    <div class="itemDetail">
      <div class="row">
        <em><xsl:value-of select="summary"/></em>
        <xsl:value-of select="detail"/>
      </div>
    </div>
  </xsl:template>

  <!--================================== Links -->

  <xsl:template match="a">
    <li>
      <xsl:choose>
        <xsl:when test="site">
          <!-- Site link -->
          <xsl:value-of select="@href"/>
          <em> @ </em>
          <a> 
            <xsl:attribute name="href">#site--<xsl:value-of select="site/@name"/></xsl:attribute>
            <xsl:value-of select="site/@name"/>
          </a>
        </xsl:when>
        <xsl:otherwise>
          <!-- Normal link -->
          <a>
            <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
            <xsl:value-of select="@href"/>
          </a>
        </xsl:otherwise>
      </xsl:choose>
    </li>
  </xsl:template>

</xsl:stylesheet>
