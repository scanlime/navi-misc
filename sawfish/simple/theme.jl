; simple/theme.jl

;; Copyright (C) 1999 Jesús Bravo Álvarez <jba@pobox.com>

(let*

  (

;; the font

    (font "-*-lucida-medium-r-normal-*-*-100-*-*-p-*-*")

;; images

    (title-images (list (make-image "titlebar-inactive.png")
                        (make-image "titlebar-active.png")))

    (s-title-images (list (make-image "titlebar-inactive-shaded.png")
                          (make-image "titlebar-active-shaded.png")))

    (title-border-images (list (make-image "titlebar-border-inactive.png")
    			       (make-image "titlebar-border-active.png")))

    (s-title-border-images (list (make-image "titlebar-border-inactive-shaded.png")
    			         (make-image "titlebar-border-active-shaded.png")))

    (iconify-images (list (make-image "iconify-inactive.png")
                          (make-image "iconify-active.png") nil
                          (make-image "iconify-pressed.png")))

    (maximize-images (list (make-image "maximize-inactive.png")
                           (make-image "maximize-active.png") nil
                           (make-image "maximize-pressed.png")))

    (close-images (list (make-image "close-inactive.png")
                        (make-image "close-active.png") nil
                        (make-image "close-pressed.png")))

    (s-close-images (list (make-image "close-inactive.png")
                          (make-image "close-active.png") nil
                          (make-image "close-pressed.png")))

    (menu-background-images (list (make-image "menu-background-inactive.png")
                          	  (make-image "menu-background-active.png") nil
	                          (make-image "menu-background-pressed.png")))

    (s-menu-background-images (list (make-image "menu-background-inactive-shaded.png")
                          	    (make-image "menu-background-active-shaded.png") nil
	                            (make-image "menu-background-pressed-shaded.png")))

    (horizontal-border-images (list (make-image "bottom-border-inactive.png")
                                    (make-image "bottom-border-active.png")))

    (left-border-images (list (make-image "left-border-inactive.png")
                                  (make-image "left-border-active.png")))

    (right-border-images (list (make-image "right-border-inactive.png")
    				  (make-image "right-border-active.png")))

    (bottom-left-images (list (make-image "bottom-left-inactive.png")
                              (make-image "bottom-left-active.png")))

    (bottom-right-images (list (make-image "bottom-right-inactive.png")
                               (make-image "bottom-right-active.png")))

    (default-icon (make-image "icon.png"))

;; frame layout

    (frame
     `(

    ;; title

        ((background . ,title-images)
         (foreground . ("white"))
         (font . ,font)
         (text . ,window-name)
         (x-justify . center)
         (y-justify . center)
         (top-edge . -19)
         (left-edge . 18)
         (right-edge . 53)
         (class . title)
        )

	((background . ,title-border-images)
	 (top-edge . -19)
	 (left-edge . 17)
	 (class . title)
	)

	((background . ,title-border-images)
	 (top-edge . -19)
	 (right-edge . 52)
	 (class . title)
	)

    ;; menu button

	((background . ,menu-background-images)
	 (top-edge . -19)
	 (left-edge . -4)
	 (class . menu-button)
	)

	((background . ,(lambda (w)
		(let ((icon (window-icon-image w)))
		  (if icon icon default-icon))))
	 (top-edge . -16)
	 (left-edge . 0)
	 (width . 14)
	 (height . 14)
	 (class . menu-button)
	)

    ;; iconify button

        ((background . ,iconify-images)
         (top-edge . -19)
         (right-edge . 33)
         (class . iconify-button)
        )

    ;; maximize button

        ((background . ,maximize-images)
         (top-edge . -19)
         (right-edge . 14)
         (class . maximize-button)
        )

    ;; close button

        ((background . ,close-images)
         (top-edge . -19)
         (right-edge . -4)
         (class . close-button)
        )

    ;; left border

        ((background . ,left-border-images)
         (top-edge . 0)
         (bottom-edge . 0)
         (left-edge . -4)
         (class . left-border)
        )

    ;; right border

        ((background . ,right-border-images)
         (top-edge . 0)
         (bottom-edge . 0)
         (right-edge . -4)
         (class . right-border)
        )

    ;; bottom border

        ((background . ,horizontal-border-images)
         (bottom-edge . -4)
         (right-edge . 0)
         (left-edge . 0)
         (class . bottom-border)
        )

    ;; bottom-left corner

        ((background . ,bottom-left-images)
         (bottom-edge . -4)
         (left-edge . -4)
         (class . bottom-left-corner)
        )

    ;; bottom-right corner

        ((background . ,bottom-right-images)
         (bottom-edge . -4)
         (right-edge . -4)
         (class . bottom-right-corner)
        )
      )
    )

    (shaped-frame
     `(

    ;; title

        ((background . ,s-title-images)
         (foreground . ("white"))
         (font . ,font)
         (text . ,window-name)
         (x-justify . center)
         (y-justify . center)
         (top-edge . -19)
         (left-edge . 18)
         (right-edge . 53)
         (class . title)
        )

	((background . ,s-title-border-images)
	 (top-edge . -19)
	 (left-edge . 17)
	 (class . title)
	)

	((background . ,s-title-border-images)
	 (top-edge . -19)
	 (right-edge . 52)
	 (class . title)
	)

    ;; menu button

	((background . ,s-menu-background-images)
	 (top-edge . -19)
	 (left-edge . -4)
	 (class . menu-button)
	)

	((background . ,(lambda (w)
		(let ((icon (window-icon-images w)))
		  (if icon icon default-icon))))
	 (top-edge . -16)
	 (left-edge . 0)
	 (width . 14)
	 (height . 14)
	 (class . menu-button)
	)

    ;; iconify button

        ((background . ,iconify-images)
         (top-edge . -19)
         (right-edge . 33)
         (class . iconify-button)
        )

    ;; maximize button

        ((background . ,maximize-images)
         (top-edge . -19)
         (right-edge . 14)
         (class . maximize-button)
        )

    ;; close button

        ((background . ,s-close-images)
         (top-edge . -19)
         (right-edge . -4)
         (class . close-button)
        )

      )
    )

    (transient-frame
     `(

    ;; title

        ((background . ,title-images)
         (foreground . ("white"))
         (font . ,font)
         (text . ,window-name)
         (x-justify . center)
         (y-justify . center)
         (top-edge . -19)
         (left-edge . 18)
         (right-edge . 16)
         (class . title)
        )

	((background . ,title-border-images)
	 (top-edge . -19)
	 (left-edge . 17)
	 (class . title)
	)

	((background . ,title-border-images)
	 (top-edge . -19)
	 (right-edge . 15)
	 (class . title)
	)

    ;; menu button

	((background . ,menu-background-images)
	 (top-edge . -19)
	 (left-edge . -4)
	 (class . menu-button)
	)

	((background . ,(lambda (w)
		(let ((icon (window-icon-image w)))
		  (if icon icon default-icon))))
	 (top-edge . -16)
	 (left-edge . 0)
	 (width . 14)
	 (height . 14)
	 (class . menu-button)
	)

    ;; close button

        ((background . ,close-images)
         (top-edge . -19)
         (right-edge . -4)
         (class . close-button)
        )

    ;; left border

        ((background . ,left-border-images)
         (top-edge . 0)
         (bottom-edge . 0)
         (left-edge . -4)
         (class . left-border)
        )

    ;; right border

        ((background . ,right-border-images)
         (top-edge . 0)
         (bottom-edge . 0)
         (right-edge . -4)
         (class . right-border)
        )

    ;; bottom border

        ((background . ,horizontal-border-images)
         (bottom-edge . -4)
         (right-edge . 0)
         (left-edge . 0)
         (class . bottom-border)
        )

    ;; bottom-left corner

        ((background . ,bottom-left-images)
         (bottom-edge . -4)
         (left-edge . -4)
         (class . bottom-left-corner)
        )

    ;; bottom-right corner

        ((background . ,bottom-right-images)
         (bottom-edge . -4)
         (right-edge . -4)
         (class . bottom-right-corner)
        )
      )
    )

    (shaped-transient-frame
     `(

    ;; title

        ((background . ,s-title-images)
         (foreground . ("white"))
         (font . ,font)
         (text . ,window-name)
         (x-justify . center)
         (y-justify . center)
         (top-edge . -19)
         (left-edge . 18)
         (right-edge . 1666666)
         (class . title)
        )

	((background . ,s-title-border-images)
	 (top-edge . -19)
	 (left-edge . 17)
	 (class . title)
	)

	((background . ,s-title-border-images)
	 (top-edge . -19)
	 (right-edge . 15)
	 (class . title)
	)

    ;; menu button

	((background . ,s-menu-background-images)
	 (top-edge . -19)
	 (left-edge . -4)
	 (class . menu-button)
	)

	((background . ,(lambda (w)
		(let ((icon (window-icon-image w)))
		  (if icon icon default-icon))))
	 (top-edge . -16)
	 (left-edge . 0)
	 (width . 14)
	 (height . 14)
	 (class . menu-button)
	)

    ;; close button

        ((background . ,s-close-images)
         (top-edge . -19)
         (right-edge . -4)
         (class . close-button)
        )
      )
    )
  )

;; frame definition

  (add-frame-style 'simple
    (lambda (w type)
      (case type
            ((default) frame)
            ((transient) transient-frame)
            ((shaped) shaped-frame)
            ((shaped-transient) shaped-transient-frame)
      )
    )
  )

)
