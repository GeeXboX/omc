SRCDIR := ..
SUBDIRS :=

include $(SRCDIR)/Makefile.common

FONTS := \
        Handycandy.ttf

PIXMAPS := \
        back_brushed2.png \
        back_brushed3.png \
        back_brushed.png \
        back_focus.png \
        background.png \
        back_menu_focus.png \
        back_menu.png \
        back_other.png \
        back.png \
        border-plain-bl.png \
        border-plain-b.png  \
        border-plain-br.png \
        border-plain-c.png \
        border-plain-l.png \
        border-plain-r.png \
        border-plain-tl.png \
        border-plain-t.png \
        border-plain-tr.png \
        border.png \
        cover-bl.png \
        cover-b.png \
        cover-br.png \
        cover-l.png \
        cover-r.png \
        cover-tl.png \
        cover-t.png \
        cover-tr.png \
        ctrl_forward_focus.png \
        ctrl_forward.png \
        ctrl_menu.png \
        ctrl_next_focus.png \
        ctrl_next.png \
        ctrl_pause_focus.png \
        ctrl_pause.png \
        ctrl_play_focus.png \
        ctrl_play.png \
        ctrl_prev_focus.png \
        ctrl_prev.png \
        ctrl_rewind_focus.png \
        ctrl_rewind.png \
        ctrl_stop_focus.png \
        ctrl_stop.png \
        dark-back2.png \
        dark-back.png \
        file_focus.png \
        file.png \
        file-selector.png \
        film_focus.png \
        film.png \
        folder.png \
        help_focus.png \
        help_icon.png \
        help.png \
        h_slider.png \
        info_focus.png \
        info_icon.png \
        info.png \
        iso.png \
        logo_dvd.png \
        logo_gx.png \
        logo_img.png \
        logo_img_small.png \
        logo_mov.png \
        logo_mov_small.png \
        logo_set.png \
        logo_set_small.png \
        logo_snd.png \
        logo_snd_small.png \
        logo_tv.png \
        logo_tv_small.png \
        menu-bl.png \
        menu-b.png \
        menu-br.png \
        menu-c.png \
        menu-l.png \
        menu-r.png \
        menu-tl.png \
        menu-t.png \
        menu-tr.png \
        music_focus.png \
        music.png \
        picture_focus.png \
        picture.png \
        playdir_focus.png \
        playdir.png \
        player_audio_focus.png \
        player_audio.png \
        playlist_focus.png \
        playlist.png \
        progress_bar.png \
        progress_border.png \
        quit_focus.png \
        quit.png \
        selector.png \
        top_border.png \
        viewer_1_1_focus.png \
        viewer_1_1.png \
        viewer_blur_focus.png \
        viewer_blur.png \
        viewer_exit_focus.png \
        viewer_exit.png \
        viewer_move_down_focus.png \
        viewer_move_down.png \
        viewer_move_left_focus.png \
        viewer_move_left.png \
        viewer_move_right_focus.png \
        viewer_move_right.png \
        viewer_move_up_focus.png \
        viewer_move_up.png \
        viewer_next_focus.png \
        viewer_next.png \
        viewer_prev_focus.png \
        viewer_prev.png \
        viewer_rotate_focus.png \
        viewer_rotate.png \
        viewer_sharpen_focus.png \
        viewer_sharpen.png \
        viewer_slideshow_focus.png \
        viewer_slideshow.png \
        viewer_zoom_fs_focus.png \
        viewer_zoom_fs.png \
        viewer_zoom_in_focus.png \
        viewer_zoom_in.png \
        viewer_zoom_out_focus.png \
        viewer_zoom_out.png \
        volume.png \
	

install:: $(FONTS) $(PIXMAPS)
	$(INSTALL) -d "$(DATA_DIR)"
	$(INSTALL) -c -m 644  $(FONTS) $(PIXMAPS) "$(DATA_DIR)"

uninstall::
	for f in $(FONTS) $(PIXMAPS); do \
		$(RM) "$(DATA_DIR)/$$f"; \
	done
