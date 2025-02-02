/**
 * Copyright (C) 2013 - 2016 Johannes Taelman
 *
 * This file is part of Axoloti.
 *
 * Axoloti is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Axoloti is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Axoloti. If not, see <http://www.gnu.org/licenses/>.
 */
package axoloti.swingui.menus;

import axoloti.swingui.CheckForUpdates;
import axoloti.swingui.dialogs.AboutFrame;
import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JMenu;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

/**
 *
 * @author jtaelman
 */
public class HelpMenu extends JMenu {

    private javax.swing.JMenuItem jMenuAbout;
    private javax.swing.JMenuItem jMenuUpdates;
    private axoloti.swingui.menus.HelpLibraryMenu helpLibraryMenu1;

    public HelpMenu() {
        initComponent();
    }

    private void initComponent() {
        addMenuListener(new MenuListener() {
            @Override
            public void menuSelected(MenuEvent e) {
                populate();
            }

            @Override
            public void menuDeselected(MenuEvent e) {
                removeAll();
            }

            @Override
            public void menuCanceled(MenuEvent e) {
                removeAll();
            }
        });
    }

    private void populate() {
        jMenuAbout = new javax.swing.JMenuItem();
        jMenuUpdates = new javax.swing.JMenuItem();
        helpLibraryMenu1 = new axoloti.swingui.menus.HelpLibraryMenu();

        jMenuAbout.setText("About");
        jMenuAbout.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuAboutActionPerformed(evt);
            }
        });
        add(jMenuAbout);

        jMenuUpdates.setText("Website");
        jMenuUpdates.addActionListener(new java.awt.event.ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuUpdatesActionPerformed(evt);
            }
        });
        add(jMenuUpdates);

        helpLibraryMenu1.setText("Library");
        add(helpLibraryMenu1);

    }

    private void jMenuAboutActionPerformed(java.awt.event.ActionEvent evt) {
        AboutFrame.aboutFrame.setVisible(true);
    }

    private void jMenuUpdatesActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            Desktop.getDesktop().browse(new URI("https://zrna.org/akso"));
        } catch (IOException ex) {
            Logger.getLogger(HelpMenu.class.getName()).log(Level.SEVERE, null, ex);
        } catch (URISyntaxException ex) {
            Logger.getLogger(HelpMenu.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
