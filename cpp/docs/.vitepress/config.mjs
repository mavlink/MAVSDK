import { defineConfig } from "vitepress";
const getSidebar = require("./get_sidebar.js");

// Tabs: https://github.com/Red-Asuka/vitepress-plugin-tabs
import tabsPlugin from "@red-asuka/vitepress-plugin-tabs";

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "MAVSDK Guide",
  description: "MAVSDK Guide",
  base: process.env.BRANCH_NAME ? "/" + process.env.BRANCH_NAME + "/" : "",
  srcExclude: [
    "de/**/*.md",
    "ja/**/*.md",
    "ru/**/*.md",
    "tr/**/*.md",
    "ko/**/*.md",
    "zh/**/*.md",
    "uk/**/*.md",
    "**/_*.md", //Remove source docs that start with "_" (included/not rendered)
    "README.md", //Remove README.md in root directory
  ],
  ignoreDeadLinks: true,
  markdown: {
    math: true,
    config: (md) => {
      // use more markdown-it plugins!
      tabsPlugin(md); //https://github.com/Red-Asuka/vitepress-plugin-tabs
    },
  },
  locales: {
    en: {
      label: "English",
      // other locale specific properties...
      themeConfig: {
        sidebar: getSidebar.sidebar("en"),

        editLink: {
          pattern: ({ filePath, frontmatter }) => {
            if (frontmatter.newEditLink) {
              //newEditLink defines a frontmatter key you can use to append a path to main
              return `https://github.com/mavlink/MAVSDK/edit/main/docs/${frontmatter.newEditLink}`;
            } else {
              return `https://github.com/mavlink/MAVSDK/edit/main/docs/${filePath}`;
            }
          },
          text: "Edit on GitHub",
        },
      },
    },

    /*
    zh: {
      label: "中文 (Chinese)",
      lang: "zh-CN", // optional, will be added  as `lang` attribute on `html` tag
      themeConfig: {
        sidebar: getSidebar.sidebar("zh"),
      },
      // other locale specific properties...
    },
    ko: {
      label: "한국어 (Korean)",
      lang: "ko-KR", // optional, will be added  as `lang` attribute on `html` tag
      themeConfig: {
        sidebar: getSidebar.sidebar("ko"),
      },

      // other locale specific properties...
    },
    */
  },

  //

  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    siteTitle: false,
    logo: {
      light: "/site/logo_mavsdk_header_lightmode.png",
      dark: "/site/logo_mavsdk_header_darkmode.png",
      alt: "",
    },
    sidebar: getSidebar.sidebar("en"),
    externalLinkIcon: true,

    editLink: {
      pattern: "https://crowdin.com/project/mavsdk",
      text: "Edit translation on Crowdin",
    },

    search: {
      provider: "local",
    },

    nav: [
      {
        text: "MAVSDK",
        items: [
          {
            text: "Website",
            link: "https://dronecode.org/sdk/",
            ariaLabel: "MAVSDK website link",
          },
          {
            text: "Source Code",
            link: "https://github.com/mavlink/MAVSDK",
          },
        ],
      },
      {
        text: "Dronecode",
        items: [
          {
            text: "PX4",
            link: "https://px4.io/",
          },
          {
            text: "QGroundControl",
            link: "http://qgroundcontrol.com/",
          },
          {
            text: "MAVLINK",
            link: "https://mavlink.io/en/",
          },
          {
            text: "PX4 Guide",
            link: "https://docs.px4.io/main/en/index.html",
          },
          {
            text: "QGroundControl Guide",
            link: "https://docs.qgroundcontrol.com/master/en/qgc-user-guide/",
          },
          {
            text: "Dronecode Camera Manager",
            link: "https://camera-manager.dronecode.org/en/",
          },
        ],
      },
      {
        text: "Support",
        link: "https://discuss.px4.io/c/mavsdk/19",
      },
      {
        text: "Version",
        items: [
          {
            text: "main / v3",
            link: "https://mavsdk.mavlink.io/main/en/cpp/api_changes.html",
          },
          {
            text: "v2",
            link: "https://mavsdk.mavlink.io/v2.0/en/cpp/api_changes.html",
          },
          {
            text: "v1.4",
            link: "https://mavsdk.mavlink.io/v1.4/en/cpp/api_changes.html",
          },
        ],
      },
    ],

    socialLinks: [
      { icon: "github", link: "https://github.com/mavlink/MAVSDK" },
    ],
  },

  head: [
    ["link", { rel: "icon", href: "/en/favicon.ico" }],
    [
      "script",
      {
        async: "",
        src: "https://www.googletagmanager.com/gtag/js?id=G-91EWVWRQ93",
      },
    ],
    [
      "script",
      {},
      `window.dataLayer = window.dataLayer || [];
      function gtag(){dataLayer.push(arguments);}
      gtag('js', new Date());
      gtag('config', 'G-91EWVWRQ93');`,
    ],
  ],

  async transformPageData(pageData, { siteConfig }) {
    //Logs every page loaded on build. Good way to catch errors not caught by other things.
    console.log(pageData.filePath);

    // Global variables as frontmatter
    pageData.frontmatter.github_branch ??= process.env.BRANCH_NAME
      ? process.env.BRANCH_NAME
      : "main";
  },

  vue: {
    template: {
      compilerOptions: {
        isCustomElement: (tag) => tag === "lite-youtube",
      },
    },
  },
});
