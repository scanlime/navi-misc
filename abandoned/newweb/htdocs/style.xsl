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
        <style type="text/css" media="all"> @import url(/new/htdocs/style.css);</style>
      <xsl:choose>
        <xsl:when test="/page/@front='true'">
          <style type="text/css">body { background: #ffffff; }</style>
        </xsl:when>
      </xsl:choose>
      </head>
      <body>
        <xsl:choose>
          <xsl:when test="/page/@front='true'">
            <!--========== Front Page -->
            <h1 class="majorTitle"><xsl:value-of select="/page/heading"/></h1>
            <xsl:apply-templates select="logo"/>
          </xsl:when>
          <xsl:otherwise>
            <!--========== Normal Page -->
            <div class="heading">
              <div class="siteName">whoopie</div>
              <div class="title"><xsl:value-of select="@title"/></div>
              <div class="subtitle"><xsl:value-of select="@subtitle"/></div>
              <div class="headingTabs">
<!--                <xsl:apply-templates select="document('tabs.xml')//headingTab"/>-->
              </div>
            </div>
            <table class="columns">
              <!--====== Navigation -->
              <td class="left">
                <span class="section">navigation</span>
                <div class="section">
                  <div class="sectionTop"/>
                  <div class="row">
                    <ul>
                      <xsl:apply-templates select="document('navigation.xml')//pages/link"/>
                    </ul>
                  </div>
                </div>
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
          </xsl:otherwise>
        </xsl:choose>
      </body>
    </html>
  </xsl:template>

  <!--================================== Navigation Links -->
  <xsl:template match="/pages/link">
    <li>
      <a>
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
      <ul>
        <xsl:apply-templates select="link"/>
      </ul>
    </li>
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

  <!--================================== Random Things -->
  <xsl:template match="logo">
    <div class="centered">
      <img src="/images/web/navi_small.png" alt="Navi Logo"/>
    </div>
  </xsl:template>

</xsl:stylesheet>
