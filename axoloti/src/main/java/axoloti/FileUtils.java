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
package axoloti;

import axoloti.abstractui.DocumentWindow;
import axoloti.abstractui.DocumentWindowList;
import axoloti.preferences.Preferences;
import axoloti.swingui.patch.PatchViewSwing;
import axoloti.swingui.patchbank.PatchBank;
import java.io.File;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;

/**
 *
 * @author jtaelman
 */
public class FileUtils {

    private FileUtils() {
    }

    public final static FileFilter axpFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axp")) {
                return true;
            } else if (file.isDirectory()) {
                return true;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Patch";
        }
    };

    public final static FileFilter axhFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axh")) {
                return true;
            } else if (file.isDirectory()) {
                return true;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Help";
        }
    };

    public final static FileFilter axsFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axs")) {
                return true;
            } else if (file.isDirectory()) {
                return true;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Subpatch";
        }
    };

    public final static FileFilter axbFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axb")) {
                return true;
            } else if (file.isDirectory()) {
                return true;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Patch Bank";
        }
    };

    public final static FileFilter axoFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axo")) {
                return true;
            } else if (file.isDirectory()) {
                return true;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Object";
        }
    };

    public final static FileFilter axtFileFilter = new FileFilter() {
        @Override
        public boolean accept(File file) {
            if (file.getName().endsWith(".axt")) {
                return true;
            } else if (file.isDirectory()) {
                return false;
            }
            return false;
        }

        @Override
        public String getDescription() {
            return "Axoloti Patcher Theme";
        }
    };

    public static JFileChooser getFileChooser() {
        JFileChooser fc = new JFileChooser(Preferences.getPreferences().getCurrentFileDirectory());
        fc.setAcceptAllFileFilterUsed(false);
        fc.addChoosableFileFilter(new FileNameExtensionFilter("Axoloti Files", "axp", "axh", "axs", "axb"));
        fc.addChoosableFileFilter(axpFileFilter);
        fc.addChoosableFileFilter(axhFileFilter);
        fc.addChoosableFileFilter(axsFileFilter);
        fc.addChoosableFileFilter(axbFileFilter);
        return fc;
    }

    public static void open(JFrame frame) {
        JFileChooser fc = getFileChooser();
        int returnVal = fc.showOpenDialog(frame);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            Preferences.getPreferences().setCurrentFileDirectory(fc.getCurrentDirectory().getPath());
            Preferences.getPreferences().savePrefs();
            File f = fc.getSelectedFile();
            for (DocumentWindow dw : DocumentWindowList.getList()) {
                if (f.equals(dw.getFile())) {
                    dw.toFront();
                    return;
                }
            }
            if (axpFileFilter.accept(f)
                    || axsFileFilter.accept(f)
                    || axhFileFilter.accept(f)) {
                PatchViewSwing.openPatch(f);
                Preferences.getPreferences().addRecentFile(f.getAbsolutePath());
            } else if (axbFileFilter.accept(f)) {
                PatchBank.openPatchBankEditor(f);
                Preferences.getPreferences().addRecentFile(f.getAbsolutePath());
            }
        }
    }

}
